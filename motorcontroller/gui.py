# Import PySide and USB classes
import sys
import usb.core
from PySide.QtCore import *
from PySide.QtGui import *

# Define our USB class (mostly copied from hellousb.py)
class JoystickUSB:

    def __init__(self):
        self.SET_MODE = 0
        self.SET_TEXTURE = 1

        self.MODE_NONE = 0
        self.MODE_SPRING = 1
        self.MODE_DAMP = 2
        self.MODE_TEXTURE = 3
        self.MODE_WALL = 4

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def set_mode(self, mode):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_MODE, int(mode), int(0))
        except usb.core.USBError:
            print "Could not send SET_MODE vendor request."

    def set_texture(self, texture):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_TEXTURE, int(texture), int(0))
        except usb.core.USBError:
            print "Could not send SET_TEXTURE vendor request."

qt_app = QApplication(sys.argv)
 
class JoystickGUI(QWidget):
 
    def __init__(self):
        # Initialize the object as a QWidget and
        # set its title and minimum width
        QWidget.__init__(self)
        self.setWindowTitle('Haptic Joystick | Louis and Derek')
        self.setMinimumWidth(500)
 
        # Create the QVBoxLayout that lays out the whole form
        self.layout = QVBoxLayout()

        self.intro_label = QLabel("Select the type of feedback you want.")
        self.layout.addWidget(self.intro_label)
 
        # Create a horizontal box layout to hold the buttons
        self.button_box = QHBoxLayout()
 
        # Create the buttons
        self.modebutton0 = QPushButton('None', self)
        self.modebutton0.clicked.connect(self.changeMode)
        self.modebutton1 = QPushButton('Spring', self)
        self.modebutton1.clicked.connect(self.changeMode)
        self.modebutton2 = QPushButton('Damper', self)
        self.modebutton2.clicked.connect(self.changeMode)
        self.modebutton3 = QPushButton('Texture', self)
        self.modebutton3.clicked.connect(self.changeMode)
        self.modebutton4 = QPushButton('Wall', self)
        self.modebutton4.clicked.connect(self.changeMode)
 
        # Add it to the button box
        self.button_box.addWidget(self.modebutton0)
        self.button_box.addWidget(self.modebutton1)
        self.button_box.addWidget(self.modebutton2)
        self.button_box.addWidget(self.modebutton3)
        self.button_box.addWidget(self.modebutton4)
 
        # Add the button box to the bottom of the main VBox layout
        self.layout.addLayout(self.button_box)
        self.layout.addSpacing(20)

        # Add a slider for adjusting the spring parameter
        self.springSliderLabel = QLabel("Spring Parameter")
        self.layout.addWidget(self.springSliderLabel)
        self.springSlider = QSlider(Qt.Horizontal, self)
        self.springSlider.setMinimum(0)
        self.springSlider.setMaximum(100)
        self.layout.addWidget(self.springSlider)
        self.layout.addSpacing(20)

        # Add a slider for adjusting the damper parameter
        self.damperSliderLabel = QLabel("Damper Parameter")
        self.layout.addWidget(self.damperSliderLabel)
        self.damperSlider = QSlider(Qt.Horizontal, self)
        self.damperSlider.setMinimum(0)
        self.damperSlider.setMaximum(100)
        self.layout.addWidget(self.damperSlider)
        self.layout.addSpacing(20)

        # Add a slider for adjusting the texture spacing
        self.textureSliderLabel = QLabel("Spacing Between Texture Bumps (250-2000)")
        self.layout.addWidget(self.textureSliderLabel)
        self.textureSlider = QSlider(Qt.Horizontal, self)
        self.textureSlider.setMinimum(250)
        self.textureSlider.setMaximum(2000)
        self.layout.addWidget(self.textureSlider)
        self.layout.addSpacing(20)

        # Add a slider for adjusting the distance to the wall
        self.wallSliderLabel = QLabel("Distance to Virtual Wall (100-10000)")
        self.layout.addWidget(self.wallSliderLabel)
        self.wallSlider = QSlider(Qt.Horizontal, self)
        self.wallSlider.setMinimum(100)
        self.wallSlider.setMaximum(10000)
        self.layout.addWidget(self.wallSlider)
        self.layout.addSpacing(20)
 
        # Set the VBox layout as the window's main layout
        self.setLayout(self.layout)

        # start the USB connection
        self.dev = JoystickUSB()
 
    def changeMode(self):
        clicked = self.sender().text()
        mode = -1
        if clicked == "None":
            self.dev.set_mode(self.dev.MODE_NONE)
        elif clicked == "Spring":
            self.dev.set_mode(self.dev.MODE_SPRING)
        elif clicked == "Damper":
            self.dev.set_mode(self.dev.MODE_DAMP)
        elif clicked == "Texture":
            self.dev.set_mode(self.dev.MODE_TEXTURE)
        elif clicked == "Wall":
            self.dev.set_mode(self.dev.MODE_WALL)

    def run(self):
        # Show the form
        self.show()
        # Run the qt application
        qt_app.exec_()
 
# Create an instance of the application window and run it
app = JoystickGUI()
app.run()