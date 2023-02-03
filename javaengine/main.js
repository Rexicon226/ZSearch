const path = require("path");
let sql = `SELECT * FROM crawles LIMIT 260000`;

function editDistance(s1, s2) {
    s1 = s1.toLowerCase();
    s2 = s2.toLowerCase();

    const costs = [];
    for (let i = 0; i <= s1.length; i++) {
        let lastValue = i;
        for (let j = 0; j <= s2.length; j++) {
            if (i === 0)
                costs[j] = j;
            else {
                if (j > 0) {
                    let newValue = costs[j - 1];
                    if (s1.charAt(i - 1) !== s2.charAt(j - 1))
                        newValue = Math.min(Math.min(newValue, lastValue),
                            costs[j]) + 1;
                    costs[j - 1] = lastValue;
                    lastValue = newValue;
                }
            }
        }
        if (i > 0)
            costs[s2.length] = lastValue;
    }
    return costs[s2.length];
}

function similarity(s1, s2) {
    let longer = s1;
    let shorter = s2;
    if (s1.length < s2.length) {
        longer = s2;
        shorter = s1;
    }
    const longerLength = longer.length;
    if (longerLength === 0) {
        return 1.0;
    }
    return (longerLength - editDistance(longer, shorter)) / parseFloat(longerLength);
}

function urlMetric(row, perfect_url, prompt, second_pass) {
    let domain = (new URL(row.url));
    let used_domain = domain.hostname + domain.pathname;
    let urldata;
    if (row.url != null) {
        urldata = similarity(perfect_url, used_domain);
        if (urldata != null) {
            second_pass.push({
                url: row.url,
                title: row.title,
                desc: row.desc,
                urlcount: urldata,
                total: urldata
            });
        }
    }
    return second_pass;
}

function Search(prompt, rows) {
    let second_pass = [];
    let perfect_url = prompt + ".com";
    let startTime = new Date().getTime();

    if (rows.length > 0) {
        for (let i = 0; i < rows.length; i++) {
            let row = rows[i];
            urlMetric(row, perfect_url, prompt, second_pass);
        }
    }

    console.log(second_pass.length);
    second_pass.sort((a, b) => {
        return b.urlcount - a.urlcount;
    });

    let endTime = new Date().getTime();
    let timeDiff = endTime - startTime;
    console.log(second_pass.splice(0, 5));
    console.log("Time: " + timeDiff + "ms");
}

const prompt = process.argv[2];

if (prompt == null) {
    console.log("Please enter a prompt:\nnode main.js [prompt] [options]");
    return;
}

const sqlite3 = require('sqlite3').verbose();

const db = new sqlite3.Database(path.resolve(__dirname, 'crawler.db'));

db.all(sql, [], (err, rows) => {
    if (err) {
        throw err;
    }

    Search(prompt, rows);

});

db.close();

console.log("Loaded")
