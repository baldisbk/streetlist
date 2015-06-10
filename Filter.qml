import QtQuick 2.0

Item {
	height: 100
	property alias dmodel: districts.model
	property alias rmodel: regions.model
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Horizontal
		central: districts
		Regions {id: regions; width: 100}
		Separator {}
		Districts {id: districts; width: 300}
	}
}

