const std = @import("std");
const sqlite = @import("sqlite.zig");

pub fn main() !void {
    var db = try sqlite.Db.init(.{
        .mode = sqlite.Db.Mode{ .File = "/home/vincent/mydata.db" },
        .open_flags = .{
            .write = true,
            .create = true,
        },
        .threading_mode = .MultiThread,
    });

    const query =
        \\SELECT id, name, age, salary FROM employees WHERE age > ? AND age < ?
    ;

    var diags = sqlite.Diagnostics{};
    var stmt = db.prepareWithDiags(query, .{ .diags = &diags }) catch |err| {
        std.log.err("unable to prepare statement, got error {}. diagnostics: {s}", .{ err, diags });
        return err;
    };
    defer stmt.deinit();

    const stdout_file = std.io.getStdOut().writer();
    var bw = std.io.bufferedWriter(stdout_file);
    const stdout = bw.writer();

    try stdout.print("Run `zig build test` to run the tests.\n", .{});

    try bw.flush(); // don't forget to flush!
}

test "simple test" {
    var list = std.ArrayList(i32).init(std.testing.allocator);
    defer list.deinit(); // try commenting this out and see if zig detects the memory leak!
    try list.append(42);
    try std.testing.expectEqual(@as(i32, 42), list.pop());
}
