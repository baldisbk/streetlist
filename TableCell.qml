import QtQuick 2.0

Rectangle {
	id: host
	property string text
	border.width: 1
	Text {
		text: host.text
		anchors.fill: parent
		horizontalAlignment: Text.AlignLeft
		verticalAlignment: Text.AlignVCenter
	}
	height: 20
	color: "#00000000"
}
