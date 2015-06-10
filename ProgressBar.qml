import QtQuick 2.0

StyleItem {
	id: host
	property int percent: 0
	property string caption
	property string comment
	Rectangle {
		id: empty
		color: emptyColor
		anchors {
			top: parent.top
			right: parent.right
			bottom: parent.bottom
			left: prog.right
		}
	}
	Rectangle {
		id: prog
		color: progressColor
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
			      (caption + " (" + comment + "): " + percent + "%")
	}
}
