import QtQuick 2.0

Rectangle {
	height: 100
	property alias dmodel: districts.model
	property alias rmodel: regions.model
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Horizontal
		central: districts
		Regions {id: regions; width: 100}
		Rectangle {width: 10; height: 10; color: "#dfdfdf"}
		Districts {id: districts; width: 300}
	}
}

