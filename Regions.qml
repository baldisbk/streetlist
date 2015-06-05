import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property RegionModel model
	Table {
		anchors.fill: parent
		model: host.model
		delegate: TableRow {
			TableCell {text: name; width: 100}
			selected: model.selected
			onSelect: host.model.select(index)
		}
		contentWidth: 100
	}
}
