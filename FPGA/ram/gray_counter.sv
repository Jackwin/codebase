/*
Copyright 2022 Chunjie Wang

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is 
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

`timescale 1ps/1ps

module gray_counter #(
    parameter COUNTER_WIDTH = 8
)(
    input                       clk,
    input                       rst,
    input                       ena,
    output [COUNTER_WIDTH-1:0]  counter,
    output [COUNTER_WIDTH-1:0]  gray_counter
);

logic [COUNTER_WIDTH-1:0] cnt;

always @ (posedge clk) begin
    if (rst)
      cnt <= 'h0;
    else if(ena)
        cnt <= cnt + 1'b1;
end

assign counter_gray = cnt ^ {1'b0, cnt[COUNTER_WIDTH-1:1]};
assign counter = cnt;

end module