import QtQuick 2.0

Rectangle {
	id: host
	property int percent: 0
	property string caption
	property color fillColor: "blue"
	color: "white"
	Rectangle {
		anchors {
			top: parent.top
			left: parent.left
			bottom: parent.bottom
		}
		color: fillColor
		width: parent.width / 100 * percent
	}
	Text {
		anchors.fill: parent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment:  Text.AlignVCenter
		text: caption + ": " + percent + "%"
	}
}
