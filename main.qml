import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.0
import Streets 1.0

Window {
	id: mainwin
	visible: true
	height: Screen.desktopAvailableHeight
	width: Screen.desktopAvailableWidth

	StyleItem {id: sizer; visible: false}
	StreetDB {
		id: database
		onFinished: {
			regionmodel.init()
			districtmodel.init()
			streetmodel.init()
		}
	}

	RegionModel {id: regionmodel; host: database.streets}
	DistrictModel {id: districtmodel; host: database.streets}
	StreetModel {id: streetmodel; host: database.streets}
	HouseModel {id: housemodel; host: database.streets; street: streetlist.selected}

	Connections {
		target: regionmodel
		onSelected: districtmodel.filter(name, flag)
		onUpdated: districtmodel.refresh()
	}
	Connections {
		target: districtmodel
		onSelected: streetmodel.filter(name, flag)
		onUpdated: streetmodel.refresh()
	}

	MultiProgressBar {id: progressBars; mode: 0; anchors.fill: parent}
	MessageBox {id: messagebox; anchors.fill: parent}

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
		onError: messagebox.display(text)
	}

	StyleItem {
		id: stateHolder
		states: [
			State {
				name: "filter"
				StateChangeScript {
					script: {
						lists.visible = true
						board.central = lists
						dbpage.visible = false
						description.visible = false
						streetmap.visible = false
					}
				}
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: true}
			},
			State {
				name: "search"
				StateChangeScript {
					script: {
						lists.visible = true
						board.central = lists
						description.visible = false
						dbpage.visible = false
						streetmap.visible = false
					}
				}
				PropertyChanges {target: submenu; visible: true}
				PropertyChanges {target: filters; visible: false}
			},
			State {
				name: "map"
				StateChangeScript {
					script: {
						lists.visible = true
						streetmap.visible = true
						board.central = streetmap
						description.visible = false
						dbpage.visible = false
						lists.height = sizer.subtableSize
					}
				}
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
			},
			State {
				name: "description"
				StateChangeScript {
					script: {
						lists.visible = true
						description.visible = true
						board.central = description
						streetmap.visible = false
						dbpage.visible = false
						lists.height = sizer.subtableSize
					}
				}
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
			},
			State {
				name: "edit"
				StateChangeScript {
					script: {
						lists.visible = true
						description.visible = true
						board.central = description
						streetmap.visible = false
						dbpage.visible = false
						lists.height = sizer.subtableSize
					}
				}
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
			},
			State {
				name: "database"
				StateChangeScript {
					script: {
						dbpage.visible = true
						board.central = dbpage
						description.visible = false
						streetmap.visible = false
						lists.visible = false
					}
				}
				PropertyChanges {target: submenu; visible: false}
				PropertyChanges {target: filters; visible: false}
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
		Separator {}
		Submenu {id: submenu}
		Filter {id: filters; rmodel: regionmodel; dmodel: districtmodel}
		Separator {visible: submenu.visible || filters.visible}
		AutoLayout {
			id: board
			orientation: Qt.Vertical
			AutoLayout {
				id: lists
				orientation: Qt.Horizontal
				central: streetlist
				Streets {id: streetlist; model: streetmodel}
				Separator {}
				Houses {id: houselist; model: housemodel}
			}
			Separator {visible: lists.visible}
			DBPage {id: dbpage; db: database}
			Desc {id: description}
			Maps {id: streetmap}
		}
	}

	Connections {
		target: submenu
		onSearch: {
			if (submenu.state == "brute")
				streetmodel.bruteforce(str, streetlist.current)
		}
	}
	Connections {
		target: streetmodel
		onSelected: {
			streetlist.current = index
			houselist.current = hindex
		}
	}

	Component.onCompleted: {
		database.init()
		database.fromdb()
	}
}
