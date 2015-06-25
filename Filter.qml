import QtQuick 2.0
import TheSystem 1.0

StyleItem {
	height: subtableSize + buttonSize
	property alias dmodel: districts.model
	property alias rmodel: regions.model
	Settings {id: settings; section: "Filter"}
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
	function loadSettings() {
		if (settings.read("Flag") == null)
			return;
		regions.cX = settings.read("regionsX")
		regions.cY = settings.read("regionsY")
		districts.cX = settings.read("districtsX")
		districts.cY = settings.read("districtsY")
	}

	Component.onDestruction: {
		settings.write("Flag", 1)
		settings.write("regionsX", regions.cX)
		settings.write("regionsY", regions.cY)
		settings.write("districtsX", districts.cX)
		settings.write("districtsY", districts.cY)
	}
}

