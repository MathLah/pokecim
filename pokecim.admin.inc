<?php
/**
 * Callback for drupal_get_form from hook_menu
 */
function pokecim_admin_data($form, &$form_state) {
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => 'Synchronize data'
    );
    return $form;
}

/**
 * Submit function for pokecim_admin_data
 */
function pokecim_admin_data_submit($form, &$form_state) {
    cache_clear_all(NULL, 'cache_pokecim');
    $batch = array(
        'title' => t('Synchronizing...'),
        'operations' => pokecim_prepare_operations(),
        'finished' => 'pokecim_synchronize_finished',
        'file' => drupal_get_path('module', 'pokecim') . '/pokecim.admin.inc',
    );
    batch_set($batch);
}
/**
 * Prepare batch operations
 * 
 * @return array opetionations
 */
function pokecim_prepare_operations() {
    $pokapi = new pokecim();
    $operations = [];
    
    $pokemons = $pokapi->get_pokemons_list_generation_1();
    foreach ($pokemons as $pokemon) {
        $operations[] = array('pokecim_synchronize_pokemon', array($pokemon));
    }
    
    $types = $pokapi->get_types();
    foreach ($types as $type) {
        $operations[] = array('pokecim_synchronize_type', array($type));
    }
    
    return $operations;
}

/**
 * Operation for pokemon
 * 
 * @param array $pokemon
 * @param array $context
 */
function pokecim_synchronize_pokemon($pokemon, &$context) {
    $pokapi = new pokecim();
    $id = $pokapi->get_id_by_url($pokemon['url']);
    watchdog('Pokecim Synchronize', "pokemon $id");
    $poke_data = $pokapi->get_pokemon_by_id($id);
    $pokename = $pokapi->get_name_by_id($id);
    
    if (! isset($context['results']['pokemon'])) {
        $context['results']['pokemon'] = array();
    }
    $context['results']['pokemon'][] = $poke_data['name'];
    $context['message'] = t('Loading Pokemon "@name"', array('@name' => $poke_data['name']));
}

/**
 * Operation for type
 * 
 * @param array $type
 * @param type $context
 */
function pokecim_synchronize_type($type, &$context) {
    $pokapi = new pokecim();
    $type['data'] = $pokapi->get_type_by_id($pokapi->get_id_by_url($type['url']));
    $pokapi->set_french_name($type['data']);

    if (! isset($context['results']['type'])) {
        $context['results']['type'] = array();
    }
    $context['results']['type'][] = $type['data']['name'];
    $context['message'] = t('Loading type "@name"', array('@name' => $type['data']['name']));
}

/**
 * Callback for end of batch
 * 
 * @param type $success
 * @param type $results
 * @param type $operations
 */
function pokecim_synchronize_finished($success, $results, $operations) {
    if ($success) {
        $count = count($results['pokemon']);
        drupal_set_message("$count pokemons are synchronize.");
        
        $count = count($results['type']);
        drupal_set_message("$count types are synchronize.");
    } else {
        drupal_set_message("An error occured", 'error');
    }
    
}