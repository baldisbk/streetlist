import QtQuick 2.0

Rectangle {
	id: host
	color: "#bf000000"
	z: 150
	visible: false
	function display(txt) {console.log(txt); thetext.text = txt; visible = true}
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

