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

	Connections {
		target: database.streets
		onProgress: {
			var prg
			if (max == 0)
				prg = 0
			else {
				prg = val * 100
				prg /= max
			}
			switch (type) {
			case StreetList.RTCity:
				cityProg.percent = prg
				cityProg.comment = val + "/" + max
				break
			case StreetList.RTRegion:
				regionProg.percent = prg
				regionProg.comment = val + "/" + max
				break
			case StreetList.RTDistrict:
				districtProg.percent = prg
				districtProg.comment = val + "/" + max
				break
			case StreetList.RTStreet:
				streetProg.percent = prg
				streetProg.comment = val + "/" + max
				break
			}
		}
	}

	StreetModel {
		id: streetmodel
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
		}

		ProgressBar {
			id: cityProg
			caption: "City"
			anchors {
				left: parent.left
				right: parent.right
			}
			height: 50
		}
		ProgressBar {
			id: regionProg
			caption: "Regions"
			anchors {
				left: parent.left
				right: parent.right
			}
			height: 50
		}
		ProgressBar {
			id: districtProg
			caption: "Districts"
			anchors {
				left: parent.left
				right: parent.right
			}
			height: 50
		}
		ProgressBar {
			id: streetProg
			percent: 0
			caption: "Streets"
			anchors {
				left: parent.left
				right: parent.right
			}
			height: 50
		}
	}

	Component.onCompleted: {database.init()}

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
