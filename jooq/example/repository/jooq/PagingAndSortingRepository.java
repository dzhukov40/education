package com.engpie.eptrunkimpl.repository.jooq;

import org.springframework.data.domain.Page;
import org.springframework.data.domain.Pageable;
import org.springframework.data.domain.Sort;

/**
 * TODO: add description.
 * NOTE: use [ org.springframework.data ]
 *
 * @author : imba
 **/
public interface PagingAndSortingRepository<T, ID> {

  Page<T> findAll(Pageable pageable);

  Iterable<T> findAll(Sort sort);

}
