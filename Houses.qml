import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias maxWidth: table.contentWidth
	property alias model: table.model
	width: maxWidth
	Table {
		id: table
		anchors.fill: parent
		delegate: TableRow {TableCell {text: name; width: fontSize * 10}}
		contentWidth: fontSize * 10
	}
}
