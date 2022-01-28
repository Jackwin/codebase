/*
Copyright 2021 Chunjie Wang

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
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FRAM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

`timescale 1ps/1ps

module ram #(
    parameter RAM_ADDR_WIDTH = 8,
    parameter RAM_DATA_WIDTH = 32
) (
    input                         clk,
    input [RAM_DATA_WIDTH-1:0]    i_wr_data,
    input                         i_we,
    input [RAM_ADDR_WIDTH-1:0]    i_addr,

    output [RAM_DATA_WIDTH-1:0]   o_rd_data
);

localparam RAM_DEPTH = 2 ** RAM_DATA_WIDTH;

logic [RAM_DATA_WIDTH-1:0] ram_bank [RAM_DEPTH-1:0];


always @(posedge clk) begin
    if (i_we) begin
        ram_bank[i_addr] <= i_wr_data;
    end
end

assign o_rd_data = ram_bank[i_addr];
    
endmodule