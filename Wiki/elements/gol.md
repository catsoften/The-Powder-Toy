# NEW GOL

## Converting GOL rules
Here's a helpful JS function to convert a GOL rule into TPT's array format:
```js
function rule_to_array_rule(rule) {
    rule = rule.split('/');
    let born = rule[0].slice(1);
    let live = rule[1].slice(1);
    let longest = born.length > live.length ? born : live;    

    r = '';    
    for (let i = 0; i < 9; i++) {
        let a = born.includes(i);
        let b = live.includes(i);
        if (a && b) r += '3';
        else if (a) r += '2';
        else if (b) r += '1';
        else r += '0';
    }
    return '{' + r.split('').join(',') + ',' + (rule[2] || 2) + '}';
}

// Example use: rule_to_array_value('B123/S456');
```

The numbers after the B indicate that a cell will be born if it has that many neighbours. The numbers after the S mean 
the cell won't die if it has that many neighbours.

## LWDH
### Life Without Death: B3/S012345678
Constantly expanding organic blob in a coral like pattern.

## SERV
### Serviettes: B234/S
Persian rug pattern, with a somewhat organized border and a messy interior.

## VT45
### Vote 4/5: B4678/S35678
Slowly decaying blob, although some stable configurations exist.

## FMZE
### Flashing Maze: B24/S1234
Maze, but the outer edges are constantly fluctuating before stabilizing, giving it a flashing seizure-inducing pattern.

## LFOD
### Live Free or Die: B2/S0
Random noise that expands in a neat diamond pattern.

## ANTI
### Anti Life: B0123478/S01234678
Square blob with a organic pattern in the interior and a cell wall appearance.

## INVL
### Inverse Life: B012345678/S34678
Grows in a square blob with constantly shrinking right angle patterns.

## HTRE
### H-Trees: B1/S012345678
Square fractal that keeps growing.

## IBAL
### Iceballs: B25678/S5678
Slowly growing inner blob with a noisy expanding outer blob.

## RULH
### Rule H: B2/S34
Random noise that expands in a neat diamond pattern.

## CRCF
### Corrosion of Conformity: B3/S124
Grows in a cool brain like pattern.

## GEMS
### Gems: B3457/S4568
Grows into stable diamond gem shapes.

## PLIF
### Pseudo GOL 2: B34/S23
Looks like normal GOL but more interconnected.

## DOTS
### DotLife: B3/S023
Also looks like normal GOL but on a larger scale.
