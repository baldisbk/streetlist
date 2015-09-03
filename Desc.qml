import QtQuick 2.0
import QtQuick.Controls 1.3
import TheSystem 1.0
import Streets 1.0

StyleItem {
	property alias street: loader.street
	WikiLoader {
		id: loader
		onStreetChanged: {
			// set progress indicator
		}
		onHtmlChanged: {
			// set progress indicator off
		}
	}
	TextArea {
		text: loader.html
		anchors.fill: parent
		readOnly: true
		textFormat: TextEdit.RichText
	}
}

