import QtQuick 2.0
import QtQuick.Dialogs 1.0

Item {
	property StreetDB db
	property alias filePath: fileEdit.text
	FileDialog {
		id: fileopen
		selectFolder: true
		onAccepted: filePath = fileUrl
	}

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
			central: fileEdit
			Button {
				text: "Load from files"
				width: 200
				onClicked: db.fromfiles(filePath);
			}
			LineEdit {id: fileEdit; text: "/home/baldis/Documents/maps/maps/tmp/"}
			Button {
				text: "..."
				width: 50
				onClicked: fileopen.open()
			}
		}
		Item {id: filler}
		central: filler
	}
}

