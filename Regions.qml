import QtQuick 2.0
import Streets 1.0

Item {
	id: host
	property alias model: table.model
	Table {
		id: table
		anchors.fill: parent
		hasSelection: false
		delegate: TableRow {
			TableCell {text: name; width: 100}
			selected: model.selected
			onSelect: table.model.select(index)
		}
		contentWidth: 100
	}
}
