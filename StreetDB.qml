import QtQuick 2.0
import Streets 1.0
import QtQuick.LocalStorage 2.0
import TheSystem 1.0

QtObject {
	property StreetList streets: StreetList{}

	property SystemFunc system: SystemFunc {}

	property var db
	property int dbVer: 1

	signal saving
	signal loading
	signal downloading

	signal finished
	signal progress(int type, int val, int max)
	signal error(string text)

	property Connections strConn: Connections {
		target: streets
		onProgress: progress(type, val, max)
		onFinished: finished()
		onLoadError: error(errorText)
	}

	function init() {
		db = LocalStorage.openDatabaseSync("TmpDB", "1.0", "", 0);
		db.transaction(_makedb)
	}
	function dump() {db.readTransaction(_dump)}
	function todb() {saving(); db.transaction(_store); finished()}
	function fromdb() {loading(); db.readTransaction(_load); finished()}
	function fromfiles(path) {downloading(); streets.loadFiles(path)}
	function fromweb() {downloading(); streets.download()}
	function check() {streets.check()}

	function _dump(tx) {
		var i
		var str = tx.executeSql('SELECT * FROM StreetsV1')
		var dis = tx.executeSql('SELECT * FROM DistrictsV1')
		var link = tx.executeSql('SELECT * FROM LinksV1')
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
		var strts = streets.streets()
		for (i=0; i<strts.length; i++) {
			var street = streets.street(strts[i])
			console.log(street.name, street.type, street.number)
		}
		console.log('-------- Districts --------')
		var districts = streets.districts()
		var strt
		for (i=0; i<districts.length; i++) {
			var district = streets.district(districts[i])
			var strlist = district.streets()
			console.log(district.name, district.region.name)
			for (var s=0; s<strlist.length; ++s) {
				strt = streets.street(strlist[s])
				console.log("   ", strt.name, strt.number, strt.type)
			}
		}
		console.log('======== End dump ========')
	}

	function _query(tx) {
		console.log('======== Query ========')
		var cust = tx.executeSql(query.text)
		streetmodel.filt(cust.rows)
		for (i = 0; i < cust.rows.length; ++i) {
			console.log('~~~~~~~~')
			for (var prop in cust.rows.item(i))
				console.log("    ", prop, "=", cust.rows.item(i)[prop])
		}
	}

	function _store(tx) {
		var strts = streets.streets()
		var districts = streets.districts()
		var i
		tx.executeSql('DELETE FROM StreetsV1')
		tx.executeSql('DELETE FROM DistrictsV1')
		tx.executeSql('DELETE FROM LinksV1')
		progress(0, 0, strts.length)
		system.processEvents()
		for (i=0; i<strts.length; i++) {
			var street = streets.street(strts[i])
			tx.executeSql(
				'INSERT INTO StreetsV1('+
					'wname, name, sec, houses, type, number,'+
					'description, coordinates, version) '+
				'VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)',
				[street.wholeName, street.name, street.secondary,
				 street.houses, street.type, street.number,
				 street.description, street.coordinates, street.version()])
			progress(0, i+1, strts.length)
			system.processEvents()
		}
		progress(1, 0, districts.length)
		system.processEvents()
		for (i=0; i<districts.length; i++) {
			var district = streets.district(districts[i])
			var res = tx.executeSql(
				'INSERT INTO DistrictsV1(name, region) '+
				'VALUES (?, ?)',
				[district.name, district.region.name])
			var strlist = district.streets()
			for (var s=0; s<strlist.length; ++s) {
				tx.executeSql(
					'INSERT INTO LinksV1(str, dis)'+
					'SELECT uid, ? AS did FROM StreetsV1 WHERE wname=?',
					[res.insertId, strlist[s]])
			}
			progress(1, i+1, districts.length)
			system.processEvents()
		}
	}

	function _load(tx) {
		var res, i;

		streets.clear();

		res = tx.executeSql('SELECT * FROM StreetsV1')
		progress(0, 0, res.rows.length)
		system.processEvents()
		for (i = 0; i < res.rows.length; ++i) {
			streets.addStreet(res.rows.item(i));
			progress(0, i+1, res.rows.length)
			system.processEvents()
		}

		res = tx.executeSql('SELECT DISTINCT region AS name FROM DistrictsV1')
		progress(1, 0, res.rows.length)
		system.processEvents()
		for (i = 0; i < res.rows.length; ++i) {
			streets.addRegion(res.rows.item(i));
			progress(1, i+1, res.rows.length)
			system.processEvents()
		}

		res = tx.executeSql('SELECT * FROM DistrictsV1')
		progress(2, 0, res.rows.length)
		system.processEvents()
		for (i = 0; i < res.rows.length; ++i) {
			streets.addDistrict(res.rows.item(i));
			progress(2, i+1, res.rows.length)
			system.processEvents()
		}

		res = tx.executeSql(
			'SELECT StreetsV1.wname AS street, DistrictsV1.name AS district'+
			' FROM LinksV1'+
			' JOIN DistrictsV1 ON DistrictsV1.uid=dis'+
			' JOIN StreetsV1 ON StreetsV1.uid=str')
		progress(3, 0, res.rows.length)
		system.processEvents()
		for (i = 0; i < res.rows.length; ++i) {
			streets.addStreetToDistrict(res.rows.item(i));
			progress(3, i+1, res.rows.length)
			system.processEvents()
		}
	}

	function _makedb(tx) {
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS Version(version INTEGER)')
		var res = tx.executeSql('SELECT version FROM Version')
		var ver = 0
		if (res.rows.length !== 0)
			ver = res.rows.item(0).version
		if (ver != dbVer) {
			tx.executeSql('INSERT INTO Version VALUES (?)', [dbVer])
			tx.executeSql(
				'CREATE TABLE DistrictsV1('+
				'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
				'name TEXT,'+
				'region TEXT,'+
				'city TEXT'+
				')')
			tx.executeSql(
				'CREATE TABLE StreetsV1('+
				'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
				'wname TEXT,'+
				'name TEXT,'+
				'sec TEXT,'+
				'type TEXT,'+
				'number TEXT,'+
				'houses TEXT,'+
				'description TEXT,'+
				'coordinates TEXT,'+
				'version INT'+
				')')
			tx.executeSql(
				'CREATE TABLE LinksV1('+
				'str INTEGER,'+
				'dis INTEGER'+
				')')

			// actually update...

			switch(ver) {
			case 0:
				break;
			default:;
			}
		}
	}
}

