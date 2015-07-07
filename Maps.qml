import QtQuick 2.0
import QtLocation 5.3
import QtPositioning 5.2
import Streets 1.0

StyleItem {
	property Street street
	property string houseNum

	function center() {
		if (geocoder.status == GeocodeModel.Loading)
			geocoder.cancel()
		geocoder.query = "Москва " + street.canonical + " " + houseNum
		geocoder.update()
	}

	Plugin {
		id: mapPlugin
		allowExperimental: true
		preferred: ["osm"]
	}

	GeocodeModel {
		id: geocoder
		plugin: mapPlugin
		onStatusChanged: {
			if (status == GeocodeModel.Ready && count != 0) {
				console.log(get(0).address.text)
				map.center = get(0).coordinate
				map.pointer.coordinate = get(0).coordinate
				map.pointer.visible = true
			} else {
				map.pointer.visible = false
			}
		}
	}

	AutoLayout {
		anchors.fill: parent
		orientation: Qt.Horizontal
		central: map
		Map {
			id: map
			plugin: mapPlugin
			gesture.enabled: true
			property MapQuickItem pointer: MapQuickItem {
				anchorPoint.x: pic.width / 2
				anchorPoint.y: pic.height / 2
				sourceItem: Image {
					id: pic
					source: "qrc:/pics/Location-32.png"
				}
			}

			Component.onCompleted: {
				zoomLevel = 16.0
				map.addMapItem(pointer)
				pointer.visible = false
			}
		}
		Separator {}
		Table {
			width: fontWidth * 30
			id: table
			model: geocoder
			hasSelection: false
			delegate: TableRow {
				TableCell {
					text: geocoder.get(index>=0?index:0).address.text
					width: fontWidth * 100
				}
			}
			contentWidth: fontWidth * 100
		}
	}
}

