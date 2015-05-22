import QtQuick 2.4
import QtQuick.Window 2.2
import QtLocation 5.3
import QtPositioning 5.2
import Streets 1.0
import QtQuick.LocalStorage 2.0

Window {
	id: mainwin
	visible: true
	height: 350
	width: 300

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

	Column {
		anchors.fill: parent

		Row {
			anchors.left: parent.left
			anchors.right: parent.right
			height: 50

			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Load web"
				onClicked: database.fromweb()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Load DB"
				onClicked: database.fromdb()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Load files"
				onClicked: database.fromfiles()
			}
		}
		Row {
			anchors.left: parent.left
			anchors.right: parent.right
			height: 50

			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Save"
				onClicked: database.todb()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Check"
				onClicked: database.check()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Dump"
				onClicked: database.dump()
			}
		}

		TextInput {
			id: query
			text: "select * from TmpStr"
			height: 50
			anchors {
				left: parent.left
				right: parent.right
			}
			visible: false
		}
	}

	Component.onCompleted: database.init()

//	Plugin {
//		id: somePlugin
//		allowExperimental: true
//		preferred: ["osm"]
//	}

//	Map {
//		id: map
//		plugin: somePlugin
//		anchors.fill: parent
//		center {
//			latitude: -27
//			longitude: 153
//		}
//		zoomLevel: map.minimumZoomLevel
//		gesture.enabled: true
//	}
}
