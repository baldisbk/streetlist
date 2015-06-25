import QtQuick 2.4
import QtQuick.Window 2.2

Item {
	FontMetrics {id: fm}
	property int sSize:
		(Screen.desktopAvailableWidth < Screen.desktopAvailableHeight) ?
			Screen.desktopAvailableWidth :
			Screen.desktopAvailableHeight

	// sizes
	property int fontWidth: fm.averageCharacterWidth * 1.5
	property int buttonSize: sSize / 8
	property int buttonTextWidth: sSize / 2
	property int textlineHeight: fm.height * 2
	property int separatorSize: sSize / 50
	property int buttonRadius: sSize / 50
	property int buttonBorder: 1
	property int cellBorder: 1
	property int editorBorder: 1
	property int headerBorder: 2
	property int buttonShift: sSize / 50
	property int fullscreenBorder: sSize / 20
	property int subtableSize: textlineHeight * 5
	property int scrollerSize: fontWidth / 3

	// animation
	property int buttonAnimation: 100

	// colors
	property color separatorColor: "#dfdfdf"
	property color selectedColor: "#ffff88"
	property color backgroundColor: "#bf000000"
	property color progressColor: "blue"
	property color emptyColor: "white"
	property color transparentColor: "#00000000"
	property color buttonBorderColor: "black"
	property color buttonPressedColor: "#ffff88"
}

