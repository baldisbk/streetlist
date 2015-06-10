import QtQuick 2.0

Item {
	property alias text: txt.text
	height: 20
	Rectangle {
		id: host
		color: "#00000000"
		border.width: 1
		anchors.fill: parent
		Text {
			id: txt
			anchors.fill: parent
			horizontalAlignment: Text.AlignLeft
			verticalAlignment: Text.AlignVCenter
		}
	}
}
