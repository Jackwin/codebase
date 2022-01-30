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

/*
Single port RAM
*/

module spram #(
    parameter SPRAM_ADDR_WIDTH = 8,
    parameter SPRAM_DATA_WIDTH = 32
) (
    input                           clk,
    input                           rst,
    input [SPRAM_DATA_WIDTH-1:0]    i_wr_data,
    input                           i_wr_ena,
    input [SPRAM_ADDR_WIDTH-1:0]    i_wr_addr,
    output                          o_full,

    input [SPRAM_ADDR_WIDTH-1:0]    i_rd_addr,
    output [SPRAM_DATA_WIDTH-1:0]   o_rd_data,
    output                          o_empty
);

localparam SPRAM_DEPTH = 2 ** SPRAM_DATA_WIDTH;

logic [SPRAM_DATA_WIDTH-1:0] spram_bank [SPRAM_DEPTH-1:0];

logic [SPRAM_ADDR_WIDTH:0]    wr_addr;
logic [SPRAM_ADDR_WIDTH:0]    wr_addr_gray;
logic [SPRAM_ADDR_WIDTH:0]    rd_addr;
logic [SPRAM_ADDR_WIDTH:0]    rd_addr_gray;
logic [SPRAM_DATA_WIDTH-1:0]  rd_data;
logic [SPRAM_ADDR_WIDTH-1:0]  addr;

assign wr_addr_gray = wr_addr ^ {1'b0, wr_addr[SPRAM_ADDR_WIDTH-1:1]};
assign rd_addr_gray = rd_addr ^ {1'b0, rd_addr[SPRAM_ADDR_WIDTH-1:1]};

assign o_full = (wr_addr_gray == {~rd_addr_gray[SPRAM_ADDR_WIDTH:SPRAM_ADDR_WIDTH-1],
                                rd_addr_gray[SPRAM_ADDR_WIDTH-2:0]});
assign o_empty = (wr_addr_gray == rd_addr_gray);

assign addr = i_wr_ena ? i_wr_addr : i_rd_addr;

ram #(
    .RAM_ADDR_WIDTH(SPRAM_ADDR_WIDTH),
    .RAM_DATA_WIDTH(SPRAM_DATA_WIDTH)
) ram_inst (
    .clk(clk),
    .i_wr_data(i_wr_data),
    .i_we(i_wr_ena),
    .i_addr(addr),

    .o_rd_data(o_rd_data)
);

endmodule