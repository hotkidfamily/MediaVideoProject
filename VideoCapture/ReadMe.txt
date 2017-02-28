VideoCapture

========================================================================
Descriptor
	Video capture through Directshow or FFmpeg or Windows Media Foundation.


========================================================================
Design
	-----------
	Single video capture source support.

	-----------
	3 Queues to use video frame.

	empty -> fill -> busy -->
	£ü						£ü
	 <--------<---------<----

	 -----------
	 Buffer Pool Support.

	 -----------
	 Memory Pool Support.