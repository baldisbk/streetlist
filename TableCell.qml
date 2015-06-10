import QtQuick 2.0

StyleItem {
	property alias text: txt.text
	height: textlineSize
	Rectangle {
		id: host
		color: transparentColor
		border.width: cellBorder
		anchors.fill: parent
		Text {
			id: txt
			anchors.fill: parent
			anchors.leftMargin: fontSize / 2
			horizontalAlignment: Text.AlignLeft
			verticalAlignment: Text.AlignVCenter
		}
	}
}
