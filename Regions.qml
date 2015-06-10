import QtQuick 2.0
import Streets 1.0

StyleItem {
	id: host
	property alias model: table.model
	property alias maxWidth: table.contentWidth
	Table {
		id: table
		anchors.fill: parent
		hasSelection: false
		delegate: TableRow {
			TableCell {text: name; width: fontSize * 10}
			selected: model.selected
			onSelect: table.model.select(index)
		}
		contentWidth: fontSize * 10
	}
}
