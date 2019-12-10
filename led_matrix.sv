/////////////////////////////////////////////

// HMC E155 19 Monica Yao and Shivam Malpani

// myao@g.hmc.edu

/////////////////////////////////////////////


// The top module for driving the matrix 
module matrix_top(input logic clk, chip_select, // chip select is default high 
						input logic sck, sdi, 
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						output logic a, b, c, d,
						output logic lat, oe, led_clk);
                 
	logic [15:0] led_data;
	logic [3:0] row;
	logic [5:0] column; 
	logic sclk;	

	led_matrix matrix(sclk, chip_select, led_data, r1, g1, b1, r2, g2, b2, row, column, a, b, c,d, lat, oe, led_clk);
	spi_slave_read spi(sck, chip_select, sdi, sclk, led_data);
endmodule


module led_matrix(input logic sclk, chip_select,
						input logic [15:0] led_data, // led_data changes every cycle (it is readings from the SPI)
						output logic r1, g1, b1, 
						output logic r2, g2, b2, 
						output logic [3:0] row,
						output logic [5:0] column,
						output logic a, b, c, d,
						output logic lat, oe, led_clk);
						

	logic [2:0] rgb1, rgb2; 	

	assign rgb1 = led_data[5:3];
	assign rgb2 = led_data[2:0];

	
	always_ff@(negedge sclk, posedge chip_select)
		if(chip_select)
			begin 
				column<= 34;
				row<=0;
			end		
		else if (column >= 34)
			begin 
				column <= 0;
			end
		else 
			begin
				column <= column + 1;
				if(column==33)
					row<= row+1; 
			end

	

	always_ff @(posedge sclk, posedge chip_select)
		
		if (chip_select)
			begin 
			{r1, g1, b1} <= 0; 
				{r2, g2, b2} <= 0; 
			end
			
		else if (column < 32)
			begin 

				{r1, g1, b1} <= rgb1; 
				{r2, g2, b2} <= rgb2; 
			end
			

	always_comb 
		begin
		oe= ~(column==33);//~(column>31&& column<34);
		lat= (column==32);
		led_clk= sclk && column<32;
		end

	assign a = row[0];
	assign b = row[1];
	assign c = row[2];
	assign d = row[3];
endmodule 


module spi_slave_read(input  logic sck, chip_select, 
               input  logic sdi,
               output  logic done,
               output logic [15:0] led_data);

    logic [3:0] byte_count;

  
	 always_ff@(negedge sck, posedge chip_select)
		if (chip_select) byte_count = 4'b1111;
		else byte_count = byte_count + 4'b1;



	 // shift register for the input
    always_ff @(posedge sck) 
        led_data <= (byte_count == 0)? {15'bxxxxxxx, sdi} : {led_data[14:0], sdi};


    // it is done with the byte count hits 15 ( this is used as clk for leds )

    assign done = (byte_count == 4'd15);

endmodule
