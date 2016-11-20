<div class="container">
    <div class="row">
        <div class="col-md-3">
            <h3>Statistics</h3>
            <ul>
                <?php foreach ($pokedata['stats'] as $stat): ?>
                    <li><?php echo $stat['data']['name']; ?> : <?php echo $stat['base_stat']; ?></li>
                <?php endforeach; ?>
            </ul>
        </div>
        <div class="col-md-3">
            <h3>Types</h3>
            <ul>
                <?php foreach ($pokedata['types'] as $type): ?>
                    <li><?php echo $type['name']; ?></li>
                <?php endforeach; ?>
            </ul>
        </div>
        <div class="col-md-3">
            <h3>Abilities</h3>
            <ul>
                <?php foreach ($pokedata['abilities'] as $ability): ?>
                    <li><?php echo $ability['name']; ?></li>
                <?php endforeach; ?>
            </ul>
        </div>
        <div class="col-md-3">
            <h3>Images</h3>
            <img src="<?php echo $pokedata['sprites']['front_default']; ?>" />
            <img src="<?php echo $pokedata['sprites']['back_default']; ?>" />
        </div>
    </div>
</div>
