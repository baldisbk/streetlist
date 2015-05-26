import QtQuick 2.0

Item {
	property int orientation
	property Item central
	function init() {
		var prev
		var wasCenter = false
		for (var i = 0; i < visibleChildren.length; ++i) {
			var curr = visibleChildren[i]
			if (orientation == Qt.Vertical) {
				if (i != 0) {
					if (wasCenter)
						prev.anchors.bottom = curr.top;
					else
						curr.anchors.top = prev.bottom;

				} else
					curr.anchors.top = top
				if (i == visibleChildren.length - 1)
					curr.anchors.bottom = bottom
				else
					curr.anchors.bottom = undefined

				curr.anchors.left = left
				curr.anchors.right = right
			} else {
				if (i != 0) {
					if (wasCenter)
						prev.anchors.right = curr.left;
					else
						curr.anchors.left = prev.right
				} else
					curr.anchors.left = left
				if (i == visibleChildren.length - 1)
					curr.anchors.right = right
				else
					curr.anchors.right = undefined
				curr.anchors.top = top
				curr.anchors.bottom = bottom
			}
			if (curr === central)
				wasCenter = true
			prev = curr
		}
	}
	onVisibleChildrenChanged: init()
	Component.onCompleted: init()
}

