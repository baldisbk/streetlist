import QtQuick 2.0

Item {
	id: host
	property int percent: 0
	property string caption
	property string comment
	property alias fillColor: filled.color
	property alias emptyColor: empty.color
	Component.onCompleted: {fillColor = "blue"; emptyColor = "white"}
	Rectangle {
		id: empty
		anchors {
			top: parent.top
			right: parent.right
			bottom: parent.bottom
			left: filled.right
		}
	}
	Rectangle {
		id: filled
		anchors {
			top: parent.top
			left: parent.left
			bottom: parent.bottom
		}
		width: parent.width / 100 * percent
	}
	Text {
		anchors.fill: parent
		horizontalAlignment: Text.AlignHCenter
		verticalAlignment:  Text.AlignVCenter
		text: (comment.length == 0) ?
			      (caption + ": " + percent + "%"):
			      (caption + "(" + comment + "): " + percent + "%")
	}
}
