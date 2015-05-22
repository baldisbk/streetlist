import QtQuick 2.0
import Streets 1.0
import QtQuick.LocalStorage 2.0

QtObject {
	property StreetList streets: StreetList{}

	property var db

	signal saving
	signal loading
	signal downloading

	signal finished
	signal progress(int type, int val, int max)

	function init() {
		db = LocalStorage.openDatabaseSync("TmpDB", "1.0", "", 0);
		db.transaction(_makedb)
	}
	function dump() {db.readTransaction(_dump)}
	function todb() {saving(); db.transaction(_store); finished()}
	function fromdb() {loading(); db.readTransaction(_load); finished()}
	function fromfiles() {downloading(); streets.loadFiles()}
	function fromweb() {downloading(); streets.download()}
	function check() {streets.check()}

	function _dump(tx) {
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
		tx.executeSql('DELETE FROM TmpStr')
		tx.executeSql('DELETE FROM TmpDis')
		tx.executeSql('DELETE FROM TmpStrInDis')
		progress(0, 0, strts.length)
		for (i=0; i<strts.length; i++) {
			var street = streets.street(strts[i])
			tx.executeSql(
				'INSERT INTO TmpStr(wname, name, sec, houses, type, number) '+
				'VALUES (?, ?, ?, ?, ?, ?)',
				[street.wholeName, street.name, street.secondary,
				 street.houses, street.type, street.number])
			progress(0, i+1, strts.length)
		}
		progress(1, 0, districts.length)
		for (i=0; i<districts.length; i++) {
			var district = streets.district(districts[i])
			var res = tx.executeSql(
				'INSERT INTO TmpDis(name, region) '+
				'VALUES (?, ?)',
				[district.name, district.region.name])
			var strlist = district.streets()
			for (var s=0; s<strlist.length; ++s) {
				tx.executeSql(
					'INSERT INTO TmpStrInDis(str, dis)'+
					'SELECT uid, ? AS did FROM TmpStr WHERE wname=?',
					[res.insertId, strlist[s]])
			}
			progress(1, i+1, districts.length)
		}
	}

	function _load(tx) {
		var res, i;

		streets.clear();

		res = tx.executeSql('SELECT * FROM TmpStr')
		progress(0, 0, res.rows.length)
		for (i = 0; i < res.rows.length; ++i) {
			streets.addStreet(res.rows.item(i));
			progress(0, i+1, res.rows.length)
		}

		res = tx.executeSql('SELECT DISTINCT region AS name FROM TmpDis')
		progress(1, 0, res.rows.length)
		for (i = 0; i < res.rows.length; ++i) {
			streets.addRegion(res.rows.item(i));
			progress(1, i+1, res.rows.length)
		}

		res = tx.executeSql('SELECT * FROM TmpDis')
		progress(2, 0, res.rows.length)
		for (i = 0; i < res.rows.length; ++i) {
			streets.addDistrict(res.rows.item(i));
			progress(2, i+1, res.rows.length)
		}

		res = tx.executeSql(
			'SELECT TmpStr.wname AS street, TmpDis.name AS district'+
			' FROM TmpStrInDis'+
			' JOIN TmpDis ON TmpDis.uid=dis'+
			' JOIN TmpStr ON TmpStr.uid=str')
		progress(3, 0, res.rows.length)
		for (i = 0; i < res.rows.length; ++i) {
			streets.addStreetToDistrict(res.rows.item(i));
			progress(3, i+1, res.rows.length)
		}
	}

	function _makedb(tx) {
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpDis('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'name TEXT,'+
			'region INTEGER)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpStr('+
			'uid INTEGER PRIMARY KEY AUTOINCREMENT,'+
			'wname TEXT,name TEXT,sec TEXT,houses TEXT,type TEXT,'+
			'number TEXT)')
		tx.executeSql(
			'CREATE TABLE IF NOT EXISTS TmpStrInDis('+
			'str INTEGER,dis INTEGER)')
	}
}

