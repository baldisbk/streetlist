import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import Streets 1.0

Window {
	id: mainwin
	visible: true
	height: 600
	width: 500

	StreetDB {id: database}
	StreetModel {id: streetmodel}

	MultiProgressBar {
		id: progressBars
		mode: 0
		anchors.fill: parent
	}

	Connections {
		target: database
		onSaving: {
			if (progressBars.mode != 0) return
			progressBars.mode = 1
			progressBars.clear()
			progressBars.add("Streets")
			progressBars.add("Districts")
		}
		onLoading: {
			if (progressBars.mode != 0) return
			progressBars.mode = 2
			progressBars.clear()
			progressBars.add("Streets")
			progressBars.add("Regions")
			progressBars.add("Districts")
			progressBars.add("Links")
		}
		onDownloading: {
			if (progressBars.mode != 0) return
			progressBars.mode = 3
			progressBars.clear()
			progressBars.add("City")
			progressBars.add("Regions")
			progressBars.add("Districts")
			progressBars.add("Streets")
		}
		onFinished: progressBars.mode = 0
		onProgress: progressBars.progress(type, val, max)
	}

	Item {
		id: stateHolder
		states: [
			State {
				name: "filter"
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: true}
				PropertyChanges {target: lists; visible: true; Layout.fillWidth: true}
				PropertyChanges {target: description; visible: false}
				PropertyChanges {target: streetmap; visible: false}
			},
			State {
				name: "search"
				PropertyChanges {target: submenu; visible: true}
				PropertyChanges {target: filters; visible: false}
				PropertyChanges {target: lists; visible: true; Layout.fillWidth: true}
				PropertyChanges {target: description; visible: false}
				PropertyChanges {target: streetmap; visible: false}
			},
			State {
				name: "map"
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
				PropertyChanges {target: lists; visible: true}
				PropertyChanges {target: description; visible: false}
				PropertyChanges {target: streetmap; visible: true; Layout.fillWidth: true}
			},
			State {
				name: "description"
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
				PropertyChanges {target: lists; visible: true}
				PropertyChanges {target: description; visible: true; Layout.fillWidth: true}
				PropertyChanges {target: streetmap; visible: false}
			},
			State {
				name: "edit"
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
				PropertyChanges {target: lists; visible: true}
				PropertyChanges {target: description; visible: true; Layout.fillWidth: true}
				PropertyChanges {target: streetmap; visible: false}
			},
			State {
				name: "database"
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
				PropertyChanges {target: lists; visible: false}
				PropertyChanges {target: description; visible: false}
				PropertyChanges {target: streetmap; visible: false}
			}
		]
		state: menu.state
	}

	// page layout

	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Vertical
		central: board
		Mainmenu {id: menu}
		Submenu {id: submenu}
		Filter {id: filters}
		SplitView {
			id: board
			height: 200
			orientation: Qt.Vertical
			handleDelegate: Component {Rectangle {color: "black"; height: 5}}
			AutoLayout {
				id: lists
				orientation: Qt.Horizontal
				central: streetlist
				Table {id: streetlist}
				Houses {id: houselist}
			}
			Desc {id: description}
			Maps {id: streetmap}
		}
	}

	Component.onCompleted: {
		database.init()
		//database.fromdb()
	}
}
