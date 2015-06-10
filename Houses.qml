import QtQuick 2.0
import Streets 1.0

Item {
	id: host
	width: 100
	property alias model: table.model
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {TableCell {text: name; width: 100}}
		contentWidth: 100
	}
}
