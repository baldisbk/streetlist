import QtQuick 2.0

Rectangle {
	id: host
	property int orientation
	property Item firstItem
	property Item secondItem
	color: "black"

	Component.onCompleted: {
		if (orientation == Qt.Horizontal) {
			height = 5
		} else {
			width = 5
		}
	}
	onWidthChanged: console.log(firstItem.width, width, secondItem.width, parent.width)

	MouseArea {
		property int moveX
		property int moveY
		anchors.fill: parent
		onPressed: {
			moveX = mouse.x
			moveY = mouse.y
		}
		onPositionChanged: {
			if (host.orientation == Qt.Horizontal) {
			//	firstItem.height -= moveY - mouse.y
				secondItem.height += moveY - mouse.y
				moveY = mouse.y
			} else {
			//	firstItem.width -= moveX - mouse.x
				secondItem.width += moveX - mouse.x
				moveX = mouse.x
			}
			console.log(mouse.buttons)
		}

		hoverEnabled: pressed
	}
}

