import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import Streets 1.0

Window {
	id: mainwin
	visible: true
	height: 600
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

	// page layout

	Mainmenu {
		id: menu
		anchors {
			top: parent.top
			left: parent.left
			right: parent.right
		}
	}
	Submenu {
		id: submenu
		anchors {
			top: menu.bottom
			left: parent.left
			right: parent.right
		}
	}
	Filter {
		id: filters
		anchors {
			top: submenu.bottom
			left: parent.left
			right: parent.right
		}
	}

	SplitView {
		anchors {
			top: filters.bottom
			left: parent.left
			right: parent.right
			bottom: parent.bottom
		}
		orientation: Qt.Vertical
		handleDelegate: Component {
			Rectangle {
				color: "black"
				height: 5
			}
		}

		Item {
			id: lists
			Layout.minimumHeight: parent.height/4
			Table {
				id: streetlist
				anchors {
					left: parent.left
					top: parent.top
					bottom: parent.bottom
					right: houselist.left
				}
			}
			Houses {
				id: houselist
				anchors {
					right: parent.right
					top: parent.top
					bottom: parent.bottom
				}
			}
		}
		Desc {id: description; Layout.minimumHeight: parent.height/4}
		Maps {id: streetmap; Layout.minimumHeight: parent.height/4}
	}

	Component.onCompleted: {
		database.init()
		database.fromdb()
	}
}
