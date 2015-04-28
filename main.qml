import QtQuick 2.4
import QtQuick.Window 2.2
import QtLocation 5.3
import QtPositioning 5.2
import Streets 1.0
import QtQuick.LocalStorage 2.0

Window {
	id: mainwin
	visible: true
	height: 200
	width: 300

	StreetDownloader {
		id: dwnld
		onProgress: {
			var prg
			if (max == 0)
				prg = 0
			else {
				prg = val * 100
				prg /= max
			}
			switch (type) {
			case StreetDownloader.RTCity: cityProg.percent = prg; break
			case StreetDownloader.RTRegion: regionProg.percent = prg; break
			case StreetDownloader.RTDistrict: districtProg.percent = prg; break
			case StreetDownloader.RTStreet: streetProg.percent = prg; break
			}
		}
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
				text: "Download"
				onClicked: dwnld.loadCity()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Save"
				onClicked: mainwin.db.transaction(mainwin.store)
			}
			Button {
				itemSize: 50
				width: mainwin.width / 3
				anchors.top: parent.top
				text: "Load"
				onClicked: mainwin.db.transaction(mainwin.load)
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

	property var db

	Street {}
	District{}
	Region{}

	function store(tx) {
		var streets = dwnld.streets()
		for (var i=0; i<streets.length; i++) {
			var street = dwnld.street(streets[i])
//			tx.executeSql(
//				'INSERT INTO TmpStr(name, houses, type, num) '+
//				'VALUES (?, ?, ?, ?)',
//				[street.name, street.houses, street.type, street.num])
			console.log(street.name, street.houses, street.type, street.num)
		}
	}

	function load(tx) {
	}

	function makedb(tx) {
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpReg('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'name TEXT)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpDis('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'name TEXT,'+
			'region INTEGER)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpStr('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'name TEXT,houses TEXT,type TEXT,'+
			'num INTEGER)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpStrInDis('+
			'str INTEGER,dis INTEGER)')
	}

	Component.onCompleted: {
		db = LocalStorage.openDatabaseSync("TmpDB", "1.0", "", 0);
		db.transaction(makedb)
	}

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
