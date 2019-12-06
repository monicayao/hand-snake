/////////////////////////////////////////////

// HMC E155 19 Monica Yao and Shivam Malpani

// myao@g.hmc.edu

/////////////////////////////////////////////



module myao_final(input logic clk, reset,

						input logic sck, sdi, 

						output logic r1, g1, b1, 

						output logic r2, g2, b2, 

						output logic a, b, c, d,

						output logic lat, oe, led_clk);

                    

	logic [7:0] led_data;

	logic [3:0] row;

	logic [5:0] column; 

	logic sclk;

	

	//slow_clk sclock(clk, reset, sclk);

	//test_display test(row, column, led_data);

	

	led_matrix matrix(sclk, reset, led_data, r1, g1, b1, r2, g2, b2, row, column, a, b, c,d, lat, oe, led_clk);

	

	spi_slave_read spi(sck, reset, sdi, sclk, led_data);

	

endmodule





module led_matrix(input logic sclk, reset,

						input logic [7:0] led_data, // led_data changes every cycle (it is readings from the SPI)

						output logic r1, g1, b1, 

						output logic r2, g2, b2, 

						output logic [3:0] row,

						output logic [5:0] column,

						output logic a, b, c, d,

						output logic lat, oe, led_clk);

						

	logic [2:0] rgb1, rgb2; 

	

	assign rgb1 = led_data[5:3];

	assign rgb2 = led_data[2:0];

	

	//temp solution 

	always_ff@(negedge sclk, posedge reset)

		if(reset)

			begin 

				column<= 0;

				row<=0;

			end
		
		else if (column == 34)
			begin 
				column <= 0;
			end

		else 

			begin

				column <= column + 1;

				if(column==33)

					row<= row+1; 

			end

	

	always_ff @(posedge sclk, posedge reset)

		if (reset)

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





module slow_clk(input logic clk, reset,  

						output logic slowclk); 

		

		// use a counter to change the frequency of the slowclk 

		// with n as 17 and k as 1, f_slowclk = 40Mhz/2^17 ~305Hz 

		logic [7:0] counter; 

		

		always_ff @(posedge clk)

			if (reset) counter <= 0;

			else		counter <= counter + 1;

				  

		assign slowclk = counter[7];

endmodule 



module test_display(input logic [3:0] row, 

						  input logic [5:0] column,

							output logic [7:0] led_data); 

		

	always_comb

		if (row == 0) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b111_000;

		else if (row == 1) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b111_000;

	

		else if (row == 2) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

	

		else if (row == 4'd3) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd4) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd5) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd6) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd7) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd8) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd9) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd10) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd11) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd12) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd13) 

		if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_000;

			

		else if (row == 4'd14) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_111;

			

		else if (row == 4'd15) 

			if(column==0 || column==1 || column ==30 || column==31)

			led_data[5:0] <= 6'b111_111;

			else 

			led_data[5:0] <= 6'b000_111;

		else

			led_data[5:0] <= 6'b000_000;

			



	always_comb 

		begin

		led_data[6]= (column>=32); 

		led_data[7]= (column>=32);

		end



	//	led_data= 8'b00111_111;

							

endmodule 


module spi_slave_read(input  logic sck, reset, 

               input  logic sdi,

               output  logic done,

               output logic [7:0] led_data);



    logic [2:0] byte_count;

    

	 always_ff@(negedge sck, posedge reset)

		if (reset) byte_count = 3'b111;

		else byte_count = byte_count + 3'b1;

	 

    

	 // shift register for the input

    always_ff @(posedge sck) 

        led_data <= (byte_count == 0)? {7'bxxxxxxx, sdi} : {led_data[6:0], sdi};

    

    // it is done with the byte count hits 7 ( this is used as clk for leds )

    assign done = (byte_count == 3'd7);

	 

endmodule