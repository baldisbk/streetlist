import QtQuick 2.0

StyleItem {
	height: subtableSize + buttonSize
	property alias dmodel: districts.model
	property alias rmodel: regions.model
	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Horizontal
		central: districtarea
		AutoLayout {
			id: regionarea
			width: (regions.maxWidth < buttonSize * 2)?buttonSize * 2:regions.maxWidth
			orientation: Qt.Vertical
			central: regions
			Regions {id: regions}
			Row {
				Button {
					image: "qrc:/pics/Plus-32.png"
					onClicked: rmodel.selectAll()
				}
				Button {
					image: "qrc:/pics/Minus-32.png"
					onClicked: rmodel.selectNone()
				}
			}
		}
		Separator {}
		AutoLayout {
			id: districtarea
			orientation: Qt.Vertical
			central: districts
			Districts {id: districts}
			Row {
				Button {
					image: "qrc:/pics/Plus-32.png"
					onClicked: dmodel.selectAll()
				}
				Button {
					image: "qrc:/pics/Minus-32.png"
					onClicked: dmodel.selectNone()
				}
			}
		}
	}
}

