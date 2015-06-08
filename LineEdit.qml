import QtQuick 2.0

Rectangle {
	id: host
	border.width: 1
	property alias text: input.text
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
			focus: true
			verticalAlignment: TextEdit.AlignVCenter
			onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
		}
	}
}
