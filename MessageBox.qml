import QtQuick 2.0

Item {
	id: host
	z: 150
	visible: false
	function display(txt) {thetext.text = txt; visible = true}
	Rectangle {
		color: "#bf000000"
		anchors.fill: parent
	}
	Rectangle {
		height: 300
		width: parent.width - 20
		anchors.centerIn: parent
		AutoLayout {
			orientation: Qt.Vertical
			anchors.fill: parent
			central: thetext
			Text {
				id: thetext
				wrapMode: Text.WordWrap
				horizontalAlignment: Text.AlignHCenter
				verticalAlignment: Text.AlignVCenter
			}
			Button {
				text: "OK"
				height: 100
				onClicked: host.visible = false
			}
		}
	}
}

