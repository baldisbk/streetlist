import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	width: 100
	property HouseModel model
	signal selected(string wname)
	Table {
		anchors.fill: parent
		model: host.model
		delegate: TableRow {
			TableCell {text: name; width: 100}
		}
		contentWidth: 100
	}
}
