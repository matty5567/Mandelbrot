

void calcMandelRow(int win_y, int win_width, int win_height, unsigned int* pixel, double x_center, double y_center, double scale_x, double scale_y) {
	int max_iter = 1000;

	double y0 =  ((win_height / 2) - win_y) * scale_y - y_center;

	for (int win_x = 0; win_x < win_width; win_x++){

		
		double x0 = x_center +  (win_x - (win_width / 2))  * scale_x;
		
		double x = 0;
		double y = 0;
		double x2 = 0;
		double y2 = 0;
		int iteration = 0;
		while ((x2 + y2 <= 4) && iteration < max_iter) {
		
			y = 2 * x * y + y0;
			x = x2 - y2 + x0;
			x2 = x * x;
			y2 = y * y;
			iteration++;
		}

		int red = 100 * iteration * iteration / max_iter * max_iter; //iteration * 255 / max_iter;
		int blue = 255 * iteration * iteration / max_iter * max_iter;
		int green = 50 * iteration * iteration / max_iter * max_iter; //iteration * 255 / max_iter;




		*pixel++ = red << 16 | green << 8 | blue;

		

	}

};
