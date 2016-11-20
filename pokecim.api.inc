<?php
/**
 * Handle the pokemon api
 */
class pokecim {
    /**
     * Prepare an url to detail page
     * 
     * @param string $url
     * @return string
     */
    function get_detail_url_from_api_url($url) {
        $id = $this->get_id_by_url($url);
        return 'pokecim/pokedeck/' . $id;
    }
    
    /**
     * Prepare an url to detail page
     * 
     * @param string $url
     * @return string
     */
    function get_detail_url_from_name($name) {
        return 'pokecim/pokedeck/' . $name;
    }
    
    /**
     * Get an Id from an URL
     * 
     * @param string $url
     * @return string
     */
    function get_id_by_url($url) {
        // remove last /
        $url = trim($url, '/');
        // find / before pokemon id
        $pos = strrpos($url, '/') + 1;
        // get pokemon id
        $id = substr($url, $pos);
        return $id;
    }
    
    function is_empty($data) {
        if (isset($data['detail'])) {
            return true;
        }
        return false;
    }
    
    /**
     * Erase name to language name
     * 
     * @param array $data
     * @param string $language
     */
    function set_french_name(&$data, $language = 'en') {
        if (! isset($data['names'])) {
            return;
        }
        foreach($data['names'] as $name) {
            if ($name['language']['name'] == $language) {
                $data['name'] = $name['name'];
            }
        }
    }
    
    /**
     * Get the language name of a pokemon by its id
     * 
     * @param int $id
     * @param string $language
     * @return string
     */
    function get_name_by_id($id, $language = 'en') {
        $pokemon = $this->get_pokemon_species_by_id($id);
        if (isset($pokemon['detail'])) {
            return NULL;
        }
        $name = $pokemon['names'][0]['name'];
        foreach($pokemon['names'] as $name_data) {
            if ($name_data['language']['name'] == $language) {
                $name = $name_data['name'];
            }
        }
        return $name;
    }

    /**
     * Get all the pokemons from the first generation
     * 
     * @return array
     */
    function get_pokemons_list_generation_1() {
        $data = $this->get_cached_data(__FUNCTION__);
        if (isset($data['pokemon_species'])) {
            return $data['pokemon_species'];
        }
        return array();
    }

    /**
     * Get a pokemon by its id
     * 
     * @param int $id
     * @return array
     */
    function get_pokemon_by_id($id) {
        return $this->get_cached_data_with_id(__FUNCTION__, $id);
    }
    
    /**
     * Get all types
     * 
     * @return array
     */
    function get_types() {
        $data = $this->get_cached_data(__FUNCTION__);
        return $data['results'];
    }

    /**
     * Get pokemon species information by its id
     * 
     * @param int $id
     * @return array
     */
    function get_pokemon_species_by_id($id) {
        return $this->get_cached_data_with_id(__FUNCTION__, $id);
    }
    
    /**
     * Get type information by its id
     * 
     * @param int $id
     * @return array
     */
    function get_type_by_id($id) {
        return $this->get_cached_data_with_id(__FUNCTION__, $id);
    }
    
    /**
     * Get ability information by its id
     * 
     * @param int $id
     * @return array
     */
    function get_abilitie_by_id($id) {
        return $this->get_cached_data_with_id(__FUNCTION__, $id);
    }
    
    /**
     * Get statistic information by its id
     * 
     * @param int $id
     * @return arrat
     */
    function get_stat_by_id($id) {
        return $this->get_cached_data_with_id(__FUNCTION__, $id);
    }

    /**
     * Return all pokemons
     * 
     * @return array
     */
    function get_pokemons_list() {
        return $this->get_cached_data(__FUNCTION__);
    }
    
    function get_cached_data_with_id($pid, $id) {
        $cached_data = cache_get($pid . $id, 'cache_pokecim');
        if (!isset($cached_data->data)) {
            $url = sprintf($this->get_url_by_pid($pid), $id);
            $data = $this->get_data(NULL, $url);
            $result = drupal_json_decode($data);
            if (! $this->is_empty($result)) {
                cache_set($pid. $id, $data, 'cache_pokecim', CACHE_TEMPORARY);
            }
        } else {
            $data = $cached_data->data;
        }
        
        return drupal_json_decode($data);
    }
    
    function get_cached_data($pid) {
        $cached_data = cache_get($pid, 'cache_pokecim');
        if (!isset($cached_data->data)) {
            $data = $this->get_data($pid);
            $result = drupal_json_decode($data);
            if (! $this->is_empty($result)) {
                cache_set($pid, $data, 'cache_pokecim', CACHE_TEMPORARY);
            }
        } else {
            $data = $cached_data->data;
        }
        return drupal_json_decode($data);
    }

    /**
     * Get API url by function name
     * 
     * @param string $pid
     *      Procedure identifier
     * 
     * @return string
     *      API URL
     */
    function get_url_by_pid($pid) {
        $urls = [
            'get_pokemons_list' => 'http://pokeapi.co/api/v2/pokemon',
            'get_pokemons_list_generation_1' => 'http://pokeapi.co/api/v2/generation/1/',
            'get_pokemon_by_id' => 'http://pokeapi.co/api/v2/pokemon/%s/',
            'get_pokemon_species_by_id' => 'http://pokeapi.co/api/v2/pokemon-species/%s/',
            'get_type_by_id' => 'http://pokeapi.co/api/v2/type/%s/',
            'get_abilitie_by_id' => 'http://pokeapi.co/api/v2/ability/%s/',
            'get_stat_by_id' => 'http://pokeapi.co/api/v2/stat/%s/',
            'get_types' => 'http://pokeapi.co/api/v2/type',
        ];

        return $urls[$pid];
    }

    /**
     * Call the API
     * 
     * @param string $pid
     *      Procedure identifier
     * @param string $url
     *      API URL
     * 
     * @return string
     *      API JSON response
     */
    function get_data($pid, $url = NULL) {
        if ($pid != NULL) {
            $url = $this->get_url_by_pid($pid);
        }
        $ch = curl_init();
        curl_setopt($ch, CURLOPT_URL, $url); 
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, TRUE); 
        curl_setopt($ch, CURLOPT_FOLLOWLOCATION, true);
        curl_setopt($ch, CURLOPT_HTTPHEADER, array(
            'Content-Type: application/json',
            'Accept: application/json'
        ));
        $output = curl_exec($ch);
        curl_close($ch);

        return $output;
    }
}