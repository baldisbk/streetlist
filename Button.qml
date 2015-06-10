import QtQuick 2.0

Item {
	id: host
	property string text
	property string image
	property int maxShift: 10
	property bool checked: false
	property bool checkable: false
	signal clicked()
Rectangle {
	anchors.fill: parent
	color: "#00000000"

	Rectangle {
		id: bord
		anchors.fill: parent
		anchors.margins: shift
		property int shift: 0
		radius: 10
		border.color: "black"
		color: host.checked?"#FFFF88":"#00000000"
	}
	AutoLayout {
		id: alo
		anchors.fill: parent
		property bool noText: host.text.length == 0
		property bool noPic: host.image.length == 0
		orientation: Qt.Horizontal
		onNoTextChanged: if (noText) central = pic; else central = txt

		Image {
			id: pic
			visible: !alo.noPic
			anchors.margins: maxShift
			source: host.image
			fillMode: Image.PreserveAspectFit
			property double ratio:
				(sourceSize.height == 0) ? 1 :
					(sourceSize.width / sourceSize.height)
			height: alo.height - host.maxShift * 2
			width: height * ratio
		}
		Text {
			id: txt
			text: host.text
			horizontalAlignment: Text.AlignHCenter
			verticalAlignment: Text.AlignVCenter
			visible: !alo.noText
		}
	}
	MouseArea {
		id: mouseArea
		anchors.fill: parent
		onClicked: {
			if (host.checkable)
				host.checked = !host.checked
			host.clicked()
		}
	}

	states: State {
		name: "pressed"
		when: mouseArea.pressed
		PropertyChanges { target: bord; color: "#FFFF88"; shift: maxShift }
	}

	transitions: Transition {
		ParallelAnimation {
			PropertyAnimation { property: "shift"; duration: 100 }
			ColorAnimation { duration: 100 }
		}
	}
}
}
