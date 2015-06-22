import QtQuick 2.4

Rectangle {
	id: host
	border.width: editorBorder
	height: textlineHeight
	property alias text: input.text
	property alias editable: input.activeFocusOnPress
	signal enter()
	Flickable {
		id: flick
		anchors.fill: host
		contentWidth: input.paintedWidth
		contentHeight: input.paintedHeight
		flickableDirection: Flickable.HorizontalFlick
		clip: true

		function ensureVisible(r) {
			if (contentX >= r.x)
				contentX = r.x;
			else if (contentX+width <= r.x+r.width)
				contentX = r.x+r.width-width;
			if (contentY >= r.y)
				contentY = r.y;
			else if (contentY+height <= r.y+r.height)
				contentY = r.y+r.height-height;
		}

		TextEdit {
			id: input
			width: flick.width
			height: flick.height
			focus: false
			verticalAlignment: TextEdit.AlignVCenter
			onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
		}
	}
	MouseArea {
		visible: !editable
		anchors.fill: parent
		onClicked: keyboard.visible = true
	}

	NumKeyboard {
		id: keyboard
		anchors.top: parent.bottom
		visible: false
		onEnter: {visible = false; host.enter()}
		onInput: input.text = input.text + ch
		onClear: input.text = ""
		onBackspace: input.text = input.text.substring(0, input.text.length - 1)
	}
}
