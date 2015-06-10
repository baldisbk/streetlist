import QtQuick 2.0

StyleItem {
	height: subtableSize
	property alias dmodel: districts.model
	property alias rmodel: regions.model
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Horizontal
		central: districts
		Regions {id: regions; width: maxWidth}
		Separator {}
		Districts {id: districts}
	}
}

