import QtQuick 2.0

Item {
	property StreetDB db
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Vertical
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Save to DB"
				width: 200
				onClicked: db.todb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Load from DB"
				width: 200
				onClicked: db.fromdb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			Button {
				text: "Load from WWW"
				width: 200
				onClicked: db.fromweb();
			}
		}
		AutoLayout {
			orientation: Qt.Horizontal
			height: 50
			central: inputContainer
			Button {
				text: "Load from files"
				width: 200
				onClicked: db.todb();
			}
			Rectangle {
				id: inputContainer
				border.width: 3
				TextInput {
					id: fileinput
					text: ""
					anchors.fill: parent
				}
			}
		}
		Item {id: filler}
		central: filler
	}
}

