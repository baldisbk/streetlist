import QtQuick 2.0
import Streets 1.0

Rectangle {
	id: host
	property DistrictModel model
	Table {
		anchors.fill: parent
		model: host.model
		delegate: TableRow {
			TableCell {text: region; width: 100}
			TableCell {text: name; width: 300}
			selected: model.selected
			onSelect: host.model.select(index)
		}
		contentWidth: 400
	}
}
