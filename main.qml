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

	StreetList {
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
				width: mainwin.width / 4
				anchors.top: parent.top
				text: "Download"
				onClicked: dwnld.download()
			}
			Button {
				itemSize: 50
				width: mainwin.width / 4
				anchors.top: parent.top
				text: "Save"
				onClicked: mainwin.db.transaction(mainwin.store)
			}
			Button {
				itemSize: 50
				width: mainwin.width / 4
				anchors.top: parent.top
				text: "Load"
				onClicked: mainwin.db.transaction(mainwin.load)
			}
			Button {
				itemSize: 50
				width: mainwin.width / 4
				anchors.top: parent.top
				text: "Dump"
				onClicked: mainwin.db.readTransaction(mainwin.dumpDb)
			}
		}

		TextInput {
			id: query
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

	property var db

	function dumpDb(tx) {
		var i
		var str = tx.executeSql('SELECT * FROM TmpStr')
		var dis = tx.executeSql('SELECT * FROM TmpDis')
		var link = tx.executeSql('SELECT * FROM TmpStrInDis')
		console.log('======== Database ========')
		console.log('-------- Streets --------')
		for (i = 0; i < str.rows.length; ++i)
			console.log(str.rows.item(i).uid, str.rows.item(i).name, str.rows.item(i).houses, str.rows.item(i).type, str.rows.item(i).number)
		console.log('-------- Districts --------')
		for (i = 0; i < dis.rows.length; ++i)
			console.log(dis.rows.item(i).uid, dis.rows.item(i).name, dis.rows.item(i).region)
		console.log('-------- Links --------')
		for (i = 0; i < link.rows.length; ++i)
			console.log(link.rows.item(i).str, link.rows.item(i).dis)
		console.log('======== App ========')
		console.log('-------- Streets --------')
		var streets = dwnld.streets()
		var districts = dwnld.districts()
		for (i=0; i<streets.length; i++) {
			var street = dwnld.street(streets[i])
			console.log(street.name, street.houses, street.type, street.number)
		}
		console.log('-------- Districts --------')
		for (i=0; i<districts.length; i++) {
			var district = dwnld.district(districts[i])
			console.log(district.name, district.region)
			var strlist = district.streets
			for (var s=0; s<strlist.length; ++s)
				console.log('    ', strlist[s])
		}
		console.log('======== Query ========')
		var cust = tx.executeSql(query.text)
		for (i = 0; i < cust.rows.length; ++i) {
			console.log('~~~~~~~~')
			for (var prop in cust.rows.item(i))
				console.log("    ", prop, "=", cust.rows.item(i)[prop])
		}
		console.log('======== End dump ========')
	}

	function store(tx) {
		var streets = dwnld.streets()
		var districts = dwnld.districts()
		var i
		tx.executeSql('DELETE FROM TmpStr')
		tx.executeSql('DELETE FROM TmpDis')
		tx.executeSql('DELETE FROM TmpStrInDis')
		for (i=0; i<streets.length; i++) {
			var street = dwnld.street(streets[i])
			tx.executeSql(
				'INSERT INTO TmpStr(wname, name, houses, type, number) '+
				'VALUES (?, ?, ?, ?, ?)',
				[street.wholeName, street.name, street.houses, street.type, street.number])
		}
		for (i=0; i<districts.length; i++) {
			var district = dwnld.district(districts[i])
			var res = tx.executeSql(
				'INSERT INTO TmpDis(name, region) '+
				'VALUES (?, ?)',
				[district.name, district.region])
			var strlist = district.streets
			for (var s=0; s<strlist.length; ++s) {
				tx.executeSql(
					'INSERT INTO TmpStrInDis(str, dis)'+
					'SELECT uid, ? AS did FROM TmpStr WHERE wname=?',
					[res.insertId, strlist[s]])
			}
		}
	}

	function load(tx) {
		var res, i;

		res = tx.executeSql('SELECT * FROM TmpStr')
		for (i = 0; i < res.rows.length; ++i)
			dwnld.addStreet(res.rows.item(i));

		res = tx.executeSql('SELECT DISTINCT region AS name FROM TmpDis')
		for (i = 0; i < res.rows.length; ++i)
			dwnld.addRegion(res.rows.item(i));

		res = tx.executeSql('SELECT * FROM TmpDis')
		for (i = 0; i < res.rows.length; ++i)
			dwnld.addDistrict(res.rows.item(i));

		res = tx.executeSql(
			'SELECT TmpStr.wname AS street, TmpDis.name AS district'+
			' FROM TmpStrInDis'+
			' JOIN TmpDis ON TmpDis.uid=dis'+
			' JOIN TmpStr ON TmpStr.uid=str')
		for (i = 0; i < res.rows.length; ++i) {
			console.log(res.rows.item(i).street, res.rows.item(i).district)
			dwnld.addStreetToDistrict(res.rows.item(i));
		}
	}

	function makedb(tx) {
//		tx.executeSql(
//			'CREATE TABLE IF NOT EXISTS TmpReg('+
//			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
//			'name TEXT)')
//		tx.executeSql('DROP TABLE IF EXISTS TmpStr')
//		tx.executeSql('DROP TABLE IF EXISTS TmpDis')
//		tx.executeSql('DROP TABLE IF EXISTS TmpStrInDis')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpDis('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'name TEXT,'+
			'region INTEGER)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpStr('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'wname TEXT,name TEXT,houses TEXT,type TEXT,'+
			'number INTEGER)')
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
